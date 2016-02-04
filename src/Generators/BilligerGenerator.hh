<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;

class BilligerGenerator extends CSVGenerator
{
	/**
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
	{

		if($resultData instanceof RecordList)
		{
			$settings = array();

			foreach($formatSettings as $formatSetting)
			{
				$settings[$formatSetting->key] = $formatSetting->value;
			}
			$this->setDelimiter(";");

			$this->addCSVContent([
					'aid',
					'name',
					'price',
					//'link',
					'brand',
					//'ean',
					'desc',
					/*'shop_cat',
					'image',
					'dlv_time',
					'dlv_cost',
					'ppu',
					'mpnr',*/
			]);

			foreach($resultData as $item)
			{
				$data = array();

				$data['aid'] = $item->itemBase->id;
				$data['name'] = $this->getName($item, $settings);
				$data['price'] = number_format($item->variationRetailPrice->price, 2, '.', '');
				// link
				$data['brand'] = $item->itemBase->producer;
				// ean
				$data['desc'] = $this->getDescription($item, $settings);

				$this->addCSVContent(array_values($data));
			}
		}
	}

	/**
	 * Get name.
	 *
	 * @param  Record $item
	 * @param  array  $settings
	 * @return string
	 */
	private function getName(Record $item, array<string, string>$settings):string
	{
		if(array_key_exists('nameId', $settings))
		{
			switch($settings['nameId'])
			{
				case 3:
					return $item->itemDescription->name3;

				case 2:
					return $item->itemDescription->name2;

				case 1:
				default:
					return $item->itemDescription->name1;
			}
		}

		return $item->itemDescription->name1;
	}

	/**
	 * Get description.
	 *
	 * @param  Record        $item
	 * @param  array<string, string>$settings
	 * @return string
	 */
	private function getDescription(Record $item, array<string, string>$settings):string
	{
		$descriptionLength = 265;
		if(array_key_exists('descriptionLength', $settings))
		{
			$descriptionLength = $settings['descriptionLength'];
		}

		if(array_key_exists('descriptionType', $settings))
		{
				switch($settings['descriptionType'])
				{
					case 'shortDescription':
						return substr(strip_tags($item->itemDescription->shortDescription), 0, $descriptionLength);

					case 'technicalData':
						return substr(strip_tags($item->itemDescription->technicalData), 0, $descriptionLength);

					case 'description':
					default:
						return substr(strip_tags($item->itemDescription->description), 0, $descriptionLength);
				}
		}

		return substr(strip_tags($item->itemDescription->description), 0, $descriptionLength);
	}
}
