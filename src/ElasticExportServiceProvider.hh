<?hh // strict
namespace ElasticExport;

use ElasticExport\Generators\BilligerGenerator;
use Plenty\Modules\DataExchange\Services\ExportPresetContainer;
use Plenty\Plugin\DataExchangeServiceProvider;

class ElasticExportServiceProvider extends DataExchangeServiceProvider
{
	public function register():void
	{
		
	}

	public function exports(ExportPresetContainer $container):void
	{

		$container->add(
            'MnoFormat',
            [
                'itemBase'=> [
                    'id',
                    'producer',
                ],

				'itemDescription' => [
					'params' => [
						'language' => 'de', // TODO change
					],
					'fields' => [
						'name1',
						'name2',
						'name3',
						'description',
						'shortDescription',
                    	'technicalData',
					],

				],

				'variationImageList' => array(
	                'params' => array(
	                        'type' => 'all', // Imageurl1  to Imageurl5
							// 'referenceMarketplace' => get reference,
	                ),
	                'fields' => array(
	                    'imageId',
	                    'type',
	                    'fileType',
	                    'path',
	                    'position',
	                    'lastUpdateTimestamp',
	                    'createTimestamp',
	                    'cleanImageName',
	                )
	            ),

				'variationRetailPrice' => [
						'price'
				],

				'variationBarcode' => [
					'params' => [
						'EAN' => [
							'barcodeType' => 'EANGeneral',
						],
					],
					'fields' => [
						'code',
						'createdTimestamp',
						'barcodeId',
					]
				],
            ],
            BilligerGenerator::class,
            []
        );

		$container->add(

        /*
        $this->formatExportFields = array(
			'aid',
			'name',
			'price',
			'link',
			'brand',
			'ean',
			'desc',
			'shop_cat',
			'image',
			'dlv_time',
			'dlv_cost',
			'ppu',
			'mpnr',
		);
        */

			'BilligerFormat',
			[
				'itemBase'=> [
					'id',
					'producer',

				],
				'itemDescription' => [
					'name1'
				],
				'variationRetailPrice' => [
						'price'
				],
				'variationBarcode' => [
					'params' => [
						'EAN' => [
							'barcodeType' => 'EANGeneral',
						],
					],
					'fields' => [
						'code',
						'createdTimestamp',
						'barcodeId',
					]
				],

			],
			BilligerGenerator::class,
			[]
		);
	}
}
