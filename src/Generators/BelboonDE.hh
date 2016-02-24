<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Contracts\UrlBuilderRepositoryContract;

class BelboonDE extends CSVGenerator
{
    const string DELIMITER = ';';

    const string IMAGE_SIZE_WIDTH = 'width';
    const string IMAGE_SIZE_HEIGHT = 'height';

    /*
     * @var ElasticExportHelper
     */
    private ElasticExportHelper $elasticExportHelper;

  	/*
  	 * @var ArrayHelper
  	 */
  	private ArrayHelper $arrayHelper;

	  /**
     * BelboonDE constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper)
    {
        $this->elasticExportHelper = $elasticExportHelper;
        $this->arrayHelper = $arrayHelper;
    }

	/**
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(self::DELIMITER);

			$this->addCSVContent([
        'Merchant_ProductNumber',
        'EAN_Code',
        'Product_Title',
        'Brand',
        'Price',
        'Currency',
        'DeepLink_URL',
        'Image_Small_URL',
        'Image_Small_WIDTH',
        'Image_Small_HEIGHT',
        'Image_Large_URL',
        'Image_Large_WIDTH',
        'Image_Large_HEIGHT',
        'Merchant_Product_Category',
        'Keywords',
        'Product_Description_Short',
        'Product_Description_Long',
        'Shipping',
        'Availability',
        'Unit_Price',
			]);

			foreach($resultData as $item)
			{
        $data = [
            'Merchant_ProductNumber'      => $item->variationBase->id,
            'EAN_Code'                    => $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
            'Product_Title'               => $this->elasticExportHelper->getName($item, $settings, 256),
            'Brand'                       => $item->itemBase->producer,
            'Price'                       => number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
            'Currency'                    => $item->variationRetailPrice->currency,
            'DeepLink_URL'                => $this->elasticExportHelper->getUrl($item, $settings),
            'Image_Small_URL'             => $this->elasticExportHelper->getImageList($item, $settings, ';', 'preview'),
            'Image_Small_WIDTH'           => '', // TODO $this->getImageSize($this->elasticExportHelper->getMainImage($item, $settings), self::IMAGE_SIZE_WIDTH),
            'Image_Small_HEIGHT'          => '', // TODO $this->getImageSize($this->elasticExportHelper->getMainImage($item, $settings), self::IMAGE_SIZE_HEIGHT),
            'Image_Large_URL'             => $this->elasticExportHelper->getImageList($item, $settings, ';', 'normal'),
            'Image_Large_WIDTH'           => '', // TODO large image size
            'Image_Large_HEIGHT'          => '', // TODO large image size
            'Merchant_Product_Category'   => $this->elasticExportHelper->getCategory($item, $settings),
            'Keywords'                    => $item->itemDescription->keywords,
            'Product_Description_Short'   => $this->elasticExportHelper->getPreviewText($item, $settings, 256),
            'Product_Description_Long'    => $this->elasticExportHelper->getDescription($item, $settings, 256),
            'Shipping'                    => number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
            'Availability'                => $this->elasticExportHelper->getAvailability($item, $settings),
            'Unit_Price'                  => $this->elasticExportHelper->getBasePrice($item, $settings),
        ];

        $this->addCSVContent(array_values($data));
			}
		}
	}

  /**
   * Returns the specific value given by type.
   * Type values are 'width' or 'height'.
   *
   * @param string $filename
   * @param string $type
   */
  private function getImageSize(string $filename, string $type):int
  {
    $imageInformation = array();
    $imageInformation[0] = $imageInformation[1] = '';

    switch ($type)
    {
      case self::IMAGE_SIZE_WIDTH:
        return (int)$imageInformation[0];
        break;
      case self::IMAGE_SIZE_HEIGHT:
        return (int)$imageInformation[1];
        break;
    }
  }
}
